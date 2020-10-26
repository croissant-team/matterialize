package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.CvType.CV_8UC1
import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.core.TermCriteria
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.blur
import org.opencv.ml.EM
import uk.ac.ic.matterialize.util.OpenCV
import kotlin.math.PI
import kotlin.math.exp
import kotlin.math.pow

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
fun Mat.getPerPixelNeighborhoodVariances(): Array<DoubleArray> {
    val meanOfSquare = Mat()
    val mean = Mat()
    val result = Mat()
    val eightNeighborhoodSize = Size(3.0, 3.0)

    // blurring is equivalent to finding the mean
    blur(this.mul(this), meanOfSquare, eightNeighborhoodSize)

    blur(this, mean, eightNeighborhoodSize)
    val squareOfMean = mean.mul(mean)

    Core.subtract(meanOfSquare, squareOfMean, result)

    return result.toSamples().toArray()
}

fun Mat.toSamples(): Mat {
    return this.clone().reshape(1, this.width() * this.height())
}

fun Mat.toArray(): Array<DoubleArray> {
    return (0 until this.height()).map { h ->
        doubleArrayOf(
            this.get(h, 0)[0],
            this.get(h, 1)[0],
            this.get(h, 2)[0]
        )
    }.toTypedArray()
}

typealias PixelPosition = Int
val twoPiCubed = (2 * PI).pow(3)
fun pdf(l2: Double, bppVar: Double): Double {
    // return exp(-1/(2 * bppVar * bppVar) * Core.norm(sample, mean)) / kotlin.math.sqrt(twoPiCubed * bppVar * bppVar * bppVar)
    return 1 / kotlin.math.sqrt(
        (twoPiCubed * bppVar * bppVar * exp(2 * bppVar * bppVar * l2))
    )
}

private class BackgroundModel(backgroundImage: Mat) : Model {
    private val gmm: EM = EM.create()

    // K_b in the background cut paper equation (2)
    // typical values in range 10-15
    private val numComponents = 5
    private val backgroundImageSamples = backgroundImage.toSamples()
    private val backgroundImageSamplesArray = backgroundImageSamples.toArray()

    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5
    private val perPixelNeighborhoodVariances: Array<DoubleArray>

    init {
        assert(mixingFactor in 0.0..1.0)

        gmm.termCriteria = TermCriteria(
            gmm.termCriteria.type,
            10,
            gmm.termCriteria.epsilon
        )

        gmm.covarianceMatrixType = EM.COV_MAT_SPHERICAL

        gmm.clustersNumber = numComponents
        // FIXME requires sample channels to be 1 so might need to convert image
        gmm.trainEM(backgroundImageSamples)
        perPixelNeighborhoodVariances = backgroundImage.getPerPixelNeighborhoodVariances()
    }

    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(image: Mat): Mat {
        val result = Mat()
        gmm.predict(image.toSamples(), result)
        return result
    }

    fun perPixelProbs(image: Mat): Mat {
        val samples = image.toSamples()

        val probs = Mat(samples.size(), samples.type())
        for (i in 0 until backgroundImageSamples.height()) {
            val d0 = samples.get(i, 0)[0] - backgroundImageSamplesArray[i][0]
            val d1 = samples.get(i, 1)[0] - backgroundImageSamplesArray[i][1]
            val d2 = samples.get(i, 2)[0] - backgroundImageSamplesArray[i][2]
            val l2 = d0 * d0 + d1 * d1 + d2 * d2
            val pixelVariance = perPixelNeighborhoodVariances[i][0]
            val prob = pdf(l2, pixelVariance)
            probs.put(i, 0, prob)
        }

        return probs
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

    val exampleMat = Imgcodecs.imread("/home/line/usr/doc/ic/assignments/3xx_60021_segp/adobe/bgs/train2014/COCO_train2014_000000000762.jpg")!!
    Imgproc.resize(exampleMat, exampleMat, Size(320.0, 240.0))
    Imgcodecs.imwrite("cache.png", exampleMat)

    val samples = exampleMat.clone().reshape(1, exampleMat.width() * exampleMat.height())
    //for (y in 0 until samples.rows()) {
    //    print("[")
    //    for (x in 0 until samples.cols()) {
    //        print(samples.get(y, x).contentToString())
    //    }
    //    println("]")
    //}
    println(samples.channels())

    val t0 = System.currentTimeMillis()
    val globalBackgroundModel = BackgroundModel(exampleMat)
    val t1 = System.currentTimeMillis()
    globalBackgroundModel.perPixelProbs(exampleMat)
    val t2 = System.currentTimeMillis()

    println("t1 - t0: ${t1 - t0}ms")
    println("t2 - t1: ${t2 - t1}ms")


}
