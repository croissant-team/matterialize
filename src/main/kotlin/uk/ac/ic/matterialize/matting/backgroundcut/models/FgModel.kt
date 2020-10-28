package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.core.TermCriteria
import org.opencv.imgproc.Imgproc
import org.opencv.ml.EM
import uk.ac.ic.matterialize.matting.Matter

class FgModel(private val bgModel: PixelBgModel) : Matter {
    val gmm: EM = EM.create()
    private val bgThreshold = 0.95
    private val fgThreshold = 1E-20
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

    override fun backgroundMask(videoFrame: Mat): Mat {
        val image: FlatImage = Image(videoFrame).flattened
        val probs = bgModel.perPixelProbs(image)
        for (i in 0 until 100) {
            print(probs.at(i).toString() + ", ")
        }
        println("")

        val bgGmmFlatMask = Mat() // F in the paper
        Imgproc.threshold(probs.mat, bgGmmFlatMask, fgThreshold, 255.0, Imgproc.THRESH_BINARY_INV) // equation(5)
        bgGmmFlatMask.convertTo(bgGmmFlatMask, CvType.CV_8UC1)

        return bgGmmFlatMask.reshape(1, videoFrame.rows())
    }

    fun globalProbs(image: FlatImage, prevSegmentationResult: SegmentationResult): Probs {
        val sample: Mat = findFgSamples(image, prevSegmentationResult)
        gmm.trainEM(sample)
        return GMMGlobalColorModel.globalProbs(gmm, image)
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
}