package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.CvType.CV_32FC1
import org.opencv.core.CvType.CV_64FC1
import org.opencv.core.CvType.CV_8UC1
import org.opencv.core.CvType.CV_8UC3
import org.opencv.core.Mat
import org.opencv.core.Scalar
import org.opencv.core.Size
import org.opencv.core.TermCriteria
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.THRESH_BINARY_INV
import org.opencv.imgproc.Imgproc.blur
import org.opencv.ml.EM
import org.opencv.ml.EM.COV_MAT_DIAGONAL
import uk.ac.ic.matterialize.util.OpenCV
import kotlin.math.PI
import kotlin.math.exp
import kotlin.math.pow


class SegmentationResult(val image: Image, val bgMask: FlatMask) {
    companion object {
        fun empty(rows: Int, cols: Int): SegmentationResult {
            return SegmentationResult(
                Image(Mat.zeros(rows, cols, CV_8UC3)),
                FlatMask(Mat.zeros(rows * cols, 1, CV_8UC1))
            )
        }
    }
}

class BackgroundCut(backgroundImage: Mat) : Matter {

    override fun backgroundMask(videoFrame: Mat): Mat {
        TODO("Not yet implemented")
    }

    fun trimap(videoFrame: Mat): Mat {
        TODO("2010 paper")
    }
}

val twoPiCubed = (2 * PI).pow(3)

// Might wanna use DoubleArrays everywhere instead of Mat for performance issue

class FlatImage(internal val mat: Mat) {
    private val data: UByteArray
    private val numChannels = 3

    init {
        assert(mat.width() == numChannels)
        val tempData = ByteArray(numPixels() * numChannels)
        mat.get(0, 0, tempData)
        data = tempData.toUByteArray()
    }

    fun get(pixelIndex: Int, channel: Int): UByte {
        return data[pixelIndex * numChannels + channel]
    }

    fun numPixels(): Int {
        return mat.height()
    }

    fun toSamples(): Mat {
        val result = Mat()
        mat.convertTo(result, CV_32FC1)
        return result
    }
}

class FlatMask(internal val mat: Mat) {
    private val data: UByteArray
    private val numChannels = 1


    init {
        assert(mat.width() == numChannels)
        val tempData = ByteArray(mat.width() * numChannels)
        mat.get(0, 0, tempData)
        data = tempData.toUByteArray()
    }

    fun get(pixelIndex: Int): UByte {
        return data[pixelIndex]
    }
}

class PixelVariances(internal val mat: Mat) {
    fun at(pixelIndex: Int, channel: Int): Double {
        return mat.get(pixelIndex, channel)[0]
    }
}

class Probs(internal val mat: Mat) {
    fun at(pixelIndex: Int): Double {
        return mat.get(pixelIndex, 0)[0]
    }
}

class Image(internal val mat: Mat) {
     val flattened: FlatImage by lazy {
         val flatMat = mat.clone().reshape(1, mat.width() * mat.height())
         return@lazy FlatImage(flatMat)
    }

    // Returns array of per pixel variances, calculated in one the 8-neighborhood
    // of each pixel
    fun getPerPixelVariances(): PixelVariances {
        val meanOfSquare = Mat()
        val mean = Mat()
        val variances = Mat()
        val eightNeighborhoodSize = Size(3.0, 3.0)

        // blurring is equivalent to finding the mean
        blur(mat.mul(mat), meanOfSquare, eightNeighborhoodSize)

        blur(mat, mean, eightNeighborhoodSize)
        val squareOfMean = mean.mul(mean)

        Core.subtract(meanOfSquare, squareOfMean, variances)
        val numPixels = mat.width() * mat.height()

        return PixelVariances(variances.reshape(1, numPixels))
    }

    fun resized(rows: Int, cols: Int): Image {
        val result = Mat()
        Imgproc.resize(this.mat, result, Size(rows.toDouble(), cols.toDouble()))
        return Image(result)
    }

    fun downscaled(factor: Int): Image {
        val width = this.mat.width() / factor
        val height = this.mat.height() / factor

        return this.resized(width, height)
    }

    fun upscaled(factor: Int): Image {
        val width = this.mat.width() * factor
        val height = this.mat.height() * factor

        return this.resized(width, height)
    }
}


object GMMGlobalColorModel {
    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(trainedGMM: EM, image: FlatImage): Probs {
        val distribProbs = Mat()
        val result = Mat()
        val samples = image.toSamples()
        println("samples: ${samples.size()}")
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

private class ForegroundModel(private val bgModel: PixelBgModel) {
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
        gmm.covarianceMatrixType = COV_MAT_DIAGONAL
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


    fun backgroundMask(videoFrame: Mat, prevSegmentationResult: SegmentationResult): Mat {
        val image: FlatImage = Image(videoFrame).flattened
        val probs = bgModel.perPixelProbs(image)
        val bgGmmMask = Mat() // F in the paper
        Imgproc.threshold(probs.mat, bgGmmMask, fgThreshold, 255.0, THRESH_BINARY_INV) // equation(5)
        val result = Mat()
        videoFrame.copyTo(result, bgGmmMask)

        return result
    }

}

private class GlobalBgModel(private val bgImage: FlatImage) {
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
private class PixelBgModel(bgImage: Image) {
    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5
    private val perPixelVariances = bgImage.getPerPixelVariances()
    private val flatBgImage = bgImage.flattened


    fun perPixelProbs(image: FlatImage): Probs {
        val probs = Mat(Size(1.0, image.numPixels().toDouble()), CV_32FC1)

        for (pixel in 0 until image.numPixels()) {
            var l2 = 0.0
            var det = 1.0
            for (channel in 0 until 3) {
                val delta = (image.get(pixel, channel) - flatBgImage.get(pixel, channel)).toDouble()
                l2 += (delta * delta) / perPixelVariances.at(pixel, channel)
                det *= perPixelVariances.at(pixel, channel)
            }

            probs.put(pixel, 0, exp(-1 / 2 * l2) / kotlin.math.sqrt(twoPiCubed * det))
        }

        return Probs(probs)
    }

}

private class ColorTerm(bgEnergies: DoubleArray, fgEnergies: DoubleArray)


private class ColorModel(private val bgImage: Image) {
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
            pixelBgModel.perPixelProbs(image).mat, 1 - mixingFactor, 0.0, result, CV_64FC1)
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

@ExperimentalUnsignedTypes
fun main() {
    OpenCV.load()
    // create simple 2x2 picture

    val exampleMat = Imgcodecs.imread("examples/COCO_train2014_000000000762.jpg")!!
    Imgproc.resize(exampleMat, exampleMat, Size(320.0 / 2, 240.0 / 2))
    Imgcodecs.imwrite("cache.png", exampleMat)

    val samples = exampleMat.clone().reshape(1, exampleMat.width() * exampleMat.height())
    println(samples.channels())

    val t0 = System.currentTimeMillis()
    val image = Image(exampleMat)
    val colorModel = ColorModel(image)
    val t1 = System.currentTimeMillis()
    colorModel.mixProbs(image.flattened)
    val t2 = System.currentTimeMillis()

    println("t1 - t0: ${t1 - t0}ms")
    println("t2 - t1: ${t2 - t1}ms")


}
