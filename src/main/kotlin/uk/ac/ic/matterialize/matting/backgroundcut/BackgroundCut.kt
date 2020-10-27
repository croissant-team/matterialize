package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.Mat
import org.opencv.core.Size
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import uk.ac.ic.matterialize.matting.Matter
import uk.ac.ic.matterialize.util.OpenCV
import kotlin.math.PI
import kotlin.math.pow

val twoPiCubed = (2 * PI).pow(3)

// Might wanna use DoubleArrays everywhere instead of Mat for performance issue

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

class BackgroundCut(backgroundImage: Mat) : Matter {
    private val bgPixelModel = PixelBgModel(Image(backgroundImage))
    private val fgModel = FgModel(bgPixelModel)

    override fun backgroundMask(videoFrame: Mat): Mat {
        return fgModel.backgroundMask(videoFrame)
    }

    fun trimap(videoFrame: Mat): Mat {
        TODO("2010 paper")
    }
}