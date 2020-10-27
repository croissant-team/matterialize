package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Scalar
import org.opencv.core.Size
import org.opencv.core.TermCriteria
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import org.opencv.ml.EM
import uk.ac.ic.matterialize.matting.Matter
import kotlin.math.exp

object GMMGlobalColorModel {
    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(trainedGMM: EM, image: FlatImage): Probs {
        val distribProbs = Mat()
        val result = Mat()
        val samples = image.toSamples()
        trainedGMM.predict(samples, distribProbs)
        val ROW_SUM = 1
        val weights = Mat()
        Core.transpose(trainedGMM.weights, weights)
        val results = Mat()

        Core.gemm(distribProbs, weights, 1.0, Mat(), 0.0, results, 0)
        Core.reduce(results, result, ROW_SUM, Core.REDUCE_SUM)

        return Probs(results)
    }
}

class ForegroundModel(private val bgModel: PixelBgModel): Matter {
    val gmm: EM = EM.create()
    private val bgThreshold = 0.95
    private val fgThreshold = 0.05
    private val numComponents = 5

    init {
        gmm.termCriteria = TermCriteria(
            gmm.termCriteria.type,
            10,
            gmm.termCriteria.epsilon
        )
        gmm.covarianceMatrixType = EM.COV_MAT_DIAGONAL
        gmm.clustersNumber = numComponents
    }

    private fun findFgSamples(currentImage: FlatImage, prevSegmentationResult: SegmentationResult): Mat {
        val probs = bgModel.perPixelProbs(currentImage)
        val numChannels = 3

        // times 2 since we are taking samples from 2 videoframes
        val samples = UByteArray(currentImage.numPixels() * numChannels * 2)
        var numSamples = 0

        val prevBgMask = prevSegmentationResult.bgMask
        val prevBgImage: FlatImage = prevSegmentationResult.image.flattened

        for (pixel in 0 until currentImage.numPixels()) {
            if (probs.at(pixel) < fgThreshold) { // pixel is in F
                // sample pixel from current image
                val currIndex = numSamples * numChannels
                for (channel in 0 until numChannels) {
                    samples[currIndex + channel] = currentImage.get(pixel, channel)
                }
                numSamples++
                val prevIndex = numSamples * numChannels

                // if pixel is in the intersection of F and the previous segmented foreground,
                // sample pixel from previous image to enforce temporal coherence
                if (prevBgMask.get(pixel) != 0.toUByte()) {
                    // sample pixel from previous image
                    for (channel in 0 until numChannels) {
                        samples[prevIndex + channel] = prevBgImage.get(pixel, channel)
                    }
                    numSamples++
                }
            }
        }

        val samplesMat = Mat(Size(numSamples.toDouble(), numChannels.toDouble()), currentImage.mat.type())
        samplesMat.put(0, 0, samples.toByteArray(), 0, numSamples)

        return samplesMat
    }

    fun globalProbs(image: FlatImage, prevSegmentationResult: SegmentationResult): Probs {
        val sample: Mat = findFgSamples(image, prevSegmentationResult)
        gmm.trainEM(sample)
        return GMMGlobalColorModel.globalProbs(gmm, image)
    }


    override fun backgroundMask(videoFrame: Mat): Mat {
        val image: FlatImage = Image(videoFrame).flattened
        val probs = bgModel.perPixelProbs(image)

        val bgGmmFlatMask = Mat() // F in the paper
        Imgproc.threshold(probs.mat, bgGmmFlatMask, fgThreshold * 255, 255.0, Imgproc.THRESH_BINARY_INV) // equation(5)
        bgGmmFlatMask.convertTo(bgGmmFlatMask, CvType.CV_8UC1)

        return bgGmmFlatMask.reshape(1, videoFrame.rows())
    }

}

class GlobalBgModel(private val bgImage: FlatImage) {
    private val gmm = EM.create()
    // K_b in the background cut paper equation (2)
    // typical values in range 10-15
    private val numComponents = 10

    init {
        gmm.termCriteria = TermCriteria(
            gmm.termCriteria.type,
            10,
            gmm.termCriteria.epsilon
        )

        gmm.clustersNumber = numComponents
        // FIXME requires sample channels to be 1 so might need to convert image
        gmm.trainEM(bgImage.toSamples())
    }

    fun globalProbs(image: FlatImage): Probs {
        return GMMGlobalColorModel.globalProbs(gmm, image)
    }
}

@ExperimentalUnsignedTypes
class PixelBgModel(bgImage: Image) {
    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5
    private val perPixelVariances = bgImage.getPerPixelVariances()
    private val flatBgImage = bgImage.flattened

    fun pdf(l2: Double, det: Double): Double {
        val x = -1 * l2 / 2
        return exp(x) / kotlin.math.sqrt(twoPiCubed * det)
    }

    fun perPixelProbs(image: FlatImage): Probs {
        val probs = Mat(Size(1.0, image.numPixels().toDouble()), CvType.CV_32FC1)

        for (pixel in 0 until image.numPixels()) {
            var l2 = 0.0
            var l2_debug = 0.0
            var det = 1.0
            for (channel in 0 until 3) {
                val delta = image.get(pixel, channel).toDouble() - (flatBgImage.get(pixel, channel)).toDouble()
                l2_debug += delta * delta
                l2 += (delta * delta) / perPixelVariances.at(pixel, channel)
                det *= perPixelVariances.at(pixel, channel)
            }

            probs.put(pixel, 0, pdf(l2, det))
        }

        return Probs(probs)
    }

}

class ColorModel(private val bgImage: Image) {
    private val globalBgModel = GlobalBgModel(bgImage.flattened)
    private val pixelBgModel = PixelBgModel(bgImage)
    private val fgModel = ForegroundModel(pixelBgModel)
    private var prevSegResult: SegmentationResult = SegmentationResult.empty(bgImage.mat.rows(), bgImage.mat.cols())

    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5

    init {
        assert(mixingFactor in 0.0..1.0)
    }

    fun updatePrevSegResult(prevSegmentationResult: SegmentationResult) {
        prevSegResult = prevSegmentationResult
    }

    fun mixProbs(image: FlatImage): Probs {
        val result = Mat()

        // equation (4)
        Core.addWeighted(
            globalBgModel.globalProbs(image).mat, mixingFactor,
            pixelBgModel.perPixelProbs(image).mat, 1 - mixingFactor, 0.0, result, CvType.CV_64FC1
        )
        return Probs(result)
    }

    fun colorTerms(image: FlatImage): ColorTerm {
        val bgEnergiesMat = Mat()
        Core.log(mixProbs(image).mat, bgEnergiesMat)
        Core.multiply(bgEnergiesMat, Scalar(-1.0), bgEnergiesMat)

        val fgEnergiesMat = Mat()
        Core.log(fgModel.globalProbs(image, prevSegResult).mat, fgEnergiesMat)
        Core.multiply(fgEnergiesMat, Scalar(-1.0), fgEnergiesMat)

        val bgEnergies = DoubleArray(image.numPixels())
        val fgEnergies = DoubleArray(image.numPixels())
        bgEnergiesMat.get(0, 0, bgEnergies)
        fgEnergiesMat.get(0, 0, fgEnergies)

        return ColorTerm(bgEnergies, fgEnergies)
    }
}