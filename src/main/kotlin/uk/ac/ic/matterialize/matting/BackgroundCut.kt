package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.CvType.CV_32FC1
import org.opencv.core.CvType.CV_32FC3
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





val twoPiCubed = (2 * PI).pow(3)
fun pdf(l2: Double, bppVar: Double): Double {
    return exp(-1 / 2 * l2) / kotlin.math.sqrt(twoPiCubed * bppVar)
}

private class ForegroundModel(private val bgModel: BackgroundModel): Matter {
    private val gmm = EM.create()

    override fun backgroundMask(videoFrame: Mat): Mat {
        val image: FlatImage = Image(videoFrame).flatten()
        val probs = bgModel.perPixelProbs(image)
    }
}



class FlatImage(internal val mat: Mat) {
    fun get(pixelIndex: Int, channel: Int): Double {
        return mat.get(pixelIndex, channel)[0]
    }

    fun numPixels(): Int {
        return mat.height()
    }
}

class Image(internal val mat: Mat) {
    fun flatten(): FlatImage {
        val flatMat = mat.clone().reshape(1, mat.width() * mat.height())
        return FlatImage(flatMat)
    }

    // Returns array of per pixel variances, calculated in one the 8-neighborhood
    // of each pixel
    fun getPerPixelVariances(): Array<DoubleArray> {
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
        val result = Array<DoubleArray>(numPixels) { DoubleArray(3) }
        val height = variances.height()
        val width = variances.width()
        
        for (i in 0 until height) {
            for (j in 0 until width) {
                val bgrVariances = variances.get(i, j)
                result[i * width + j] = bgrVariances
            }
        }

        return result
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

// TODO should this be just a DoubleArray?
class Probs(internal val mat: Mat)

private class BackgroundModel(private val bgImage: Image) : Model {

    // shrink image to speed up computation
    private val downscaleFactor = 4


    private val gmm = EM.create()
    // K_b in the background cut paper equation (2)
    // typical values in range 10-15
    private val numComponents = 10

    private val flatBgImage = bgImage.flatten()
    // alpha in equation (4)
    // must be between 0 and 1
    private val mixingFactor = 0.5
    private val perPixelVariances: Array<DoubleArray>

    init {
        assert(mixingFactor in 0.0..1.0)

        gmm.termCriteria = TermCriteria(
            gmm.termCriteria.type,
            10,
            gmm.termCriteria.epsilon
        )

        gmm.clustersNumber = numComponents
        // FIXME requires sample channels to be 1 so might need to convert image
        gmm.trainEM(bgImage.mat)
        perPixelVariances = bgImage.getPerPixelVariances()
    }

    // TODO Check if this is incorrect, but I'm pretty sure it does exactly equation (2)
    fun globalProbs(image: FlatImage): Probs {
        val distribProbs = Mat()
        val result = Mat()

        gmm.predict(image.mat, distribProbs)
        val ROW_SUM = 1
        Core.reduce(distribProbs.mul(gmm.weights), result, ROW_SUM, CV_32FC1)


        return Probs(result)
    }

    fun perPixelProbs(image: FlatImage): Probs {
        val probs = DoubleArray(image.numPixels())

        for (pixel in 0 until image.numPixels()) {
            val pixelVariance = perPixelVariances[pixel]
            val d0 = image.get(pixel, 0) - flatBgImage.get(pixel, 0)
            val d1 = image.get(pixel, 1) - flatBgImage.get(pixel, 1)
            val d2 = image.get(pixel, 2) - flatBgImage.get(pixel, 2)
            val l2 = (d0 * d0) / pixelVariance[0] + (d1 * d1) / pixelVariance[1] + (d2 * d2) / pixelVariance[2]
            val det = pixelVariance[0] * pixelVariance[1] * pixelVariance[2]

            probs[pixel] = exp(-1 / 2 * l2) / kotlin.math.sqrt(twoPiCubed * det)
        }

        return Probs()
    }

    fun mixProbs(image: FlatImage): Probs {
        val result = Mat()

        // equation (4)
        Core.addWeighted(globalProbs(image).mat, mixingFactor, perPixelProbs(image).mat, 1 - mixingFactor, 0.0, result)
        return Probs(result)
    }
}

@ExperimentalUnsignedTypes
fun main() {
    OpenCV.load()
    // create simple 2x2 picture

    val exampleMat = Imgcodecs.imread("examples/COCO_train2014_000000000762.jpg")!!
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
