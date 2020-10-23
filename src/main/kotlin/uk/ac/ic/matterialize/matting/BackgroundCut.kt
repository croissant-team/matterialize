package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.CvType.CV_8UC1
import org.opencv.core.CvType.CV_8UC3
import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc.blur
import org.opencv.ml.EM
import uk.ac.ic.matterialize.util.OpenCV

private interface Model {

}

data class SegmentationResult(val image: Mat, val backgroundMask: Mat)

class BackgroundCut(backgroundImage: Mat) : Matter {
    private val globalBgModel: Model = BackgroundModel(backgroundImage)
    private val globalFgModelLearner = GlobalForegroundModelLearner(globalBgModel)
    private lateinit var prevSegmentationResult: SegmentationResult

    init {
        // defaults the previous image to a black image with no foreground or background
        val prevImage = Mat(backgroundImage.size(), backgroundImage.type())
        // all black mask
        val mask = Mat(backgroundImage.size(), CV_8UC1)
        SegmentationResult(prevImage, mask)
    }

    override fun backgroundMask(videoFrame: Mat): Mat {
        val globalFgModel: Model = globalFgModelLearner.learn(videoFrame, prevSegmentationResult)

        TODO("Not yet implemented")
    }

    fun trimap(videoFrame: Mat): Mat {
        TODO("2010 paper")
    }
}

private class GlobalForegroundModelLearner(perPixelBgModel: Model) {

    fun learn(videoFrame: Mat, prevSegmentationResult: SegmentationResult): Model {
        TODO("Not yet implemented")
    }
}

private class PerPixelBackgroundModel(backgroundImage: Mat) : Model {

}

// TODO find a better place for this
fun Mat.getPerPixelNeighborhoodVariances(): Mat {
    val meanOfSquare = Mat()
    val mean = Mat()
    val result = Mat()
    val eightNeighborhoodSize = Size(3.0, 3.0)

    // blurring is equivalent to finding the mean
    blur(this.mul(this), meanOfSquare, eightNeighborhoodSize)

    blur(this, mean, eightNeighborhoodSize)
    val squareOfMean = mean.mul(mean)

    Core.subtract(meanOfSquare, squareOfMean, result)

    return result
}

private class BackgroundModel(private val backgroundImage: Mat) : Model {
    private val gmm: EM = EM.create()

    // K_b in the background cut paper equation (2)
    // typical values in range 10-15
    private val numComponents = 13

    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5
    private val perPixelNeighborhoodVariances: Mat


    init {
        assert(mixingFactor in 0.0..1.0)
        gmm.clustersNumber = numComponents
        // FIXME requires sample channels to be 1 so might need to convert image
        gmm.trainEM(backgroundImage)
        perPixelNeighborhoodVariances = backgroundImage.getPerPixelNeighborhoodVariances()
    }

    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(image: Mat): Mat {
        val result = Mat()
        gmm.predict(image, result)
        return result
    }

    fun perPixelProbs(image: Mat): Mat {
        val means = backgroundImage
        TODO()

    }

    fun mixProbs(image: Mat): Mat {
        val result = Mat()

        // equation (4)
        Core.addWeighted(globalProbs(image), mixingFactor, perPixelProbs(image), 1 - mixingFactor, 0.0, result)
        return result
    }
}

@ExperimentalUnsignedTypes
fun main() {
    OpenCV.load()
    // create simple 2x2 picture
    val exampleMat = Mat(Size(2.0, 2.0), CV_8UC3)
    val green = ubyteArrayOf(0u, 255u, 0u).toByteArray()
    val red = ubyteArrayOf(0u, 0u, 255u).toByteArray()
    val blue = ubyteArrayOf(255u, 0u, 0u).toByteArray()
    val white = ubyteArrayOf(255u, 255u, 255u).toByteArray()

    exampleMat.put(0, 0, green + red + blue + white)

    val globalBackgroundModel = BackgroundModel(exampleMat)

    Imgcodecs.imwrite("cache.png", exampleMat)
}
