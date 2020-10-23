package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.image.ImageView
import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import tornadofx.Controller
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.matting.BackgroundNegationMatter
import uk.ac.ic.matterialize.matting.FaceDetectionMatter
import uk.ac.ic.matterialize.matting.KMeansMatter
import uk.ac.ic.matterialize.matting.Matter
import uk.ac.ic.matterialize.matting.MatterMode
import uk.ac.ic.matterialize.util.Converter
import java.awt.image.BufferedImage
import java.awt.image.DataBufferByte
import java.io.File
import javax.imageio.ImageIO
import kotlin.concurrent.thread

class MatterializeController : Controller() {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video100"

    val WIDTH = 640
    val HEIGHT = 480

    val inputCam = OpenCVWebcam(INPUT_DEVICE, WIDTH, HEIGHT)
    val outputCam = FakeWebcam(OUTPUT_DEVICE, WIDTH, HEIGHT)

    var matter: Matter? = null
    var background: Mat? = null

    private lateinit var inputView: ImageView
    private lateinit var outputView: ImageView

    fun setInputImage(image: ImageView) {
        inputView = image
    }

    fun setOutputImage(image: ImageView) {
        outputView = image
    }

    fun applyNewBackground(inputValue: String) {
        println("Image '$inputValue' has been selected")

        val rawBackground: BufferedImage = ImageIO.read(File(inputValue))

        val resized = BufferedImage(WIDTH, HEIGHT, rawBackground.type)
        val graphics = resized.createGraphics()
        graphics.drawImage(rawBackground, 0, 0, WIDTH, HEIGHT, null)
        graphics.dispose()

        val background = Mat(resized.height, resized.width, CvType.CV_8UC3)
        background.put(0, 0, (resized.raster.dataBuffer as DataBufferByte).data)

        this.background = background
    }

    fun setMatter(mode: MatterMode) {
        println("mode changed to $mode")
        matter = when (mode) {
            MatterMode.KMeans -> KMeansMatter(inputCam.grab())
            MatterMode.FaceDetection -> FaceDetectionMatter()
            MatterMode.BackgroundNegation -> BackgroundNegationMatter(inputCam.grab())
            MatterMode.OpenCV -> OpenCVMatter()
        }
    }

    fun joinThreads() {
        inputCam.stop()
        outputCam.stop()
        // terminate all threads
    }

    fun initialiseWebCamThread() {
        thread {
//            println("Average FPS: ${inputCam.fps(100)}")

            while (true) {
                val start = System.currentTimeMillis()

                val img = inputCam.grab()

                val mat = when {
                    matter == null || background == null -> img
                    else -> matter!!.changeBackground(img, background!!)
                }

                outputCam.write(Converter.convertToYUYV(mat))

                inputView.image = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(img), null)
                outputView.image = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(mat), null)

                val end = System.currentTimeMillis()
                println("took ${end - start}ms")
            }
        }
    }

    fun getImageWidth(): Double {
        return WIDTH.toDouble()
    }

    init {
        inputCam.start()
        outputCam.start()
    }
}
