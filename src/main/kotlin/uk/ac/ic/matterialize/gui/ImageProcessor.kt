package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.image.ImageView
import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.matting.*
import uk.ac.ic.matterialize.util.Converter
import java.awt.image.BufferedImage
import java.awt.image.DataBufferByte
import java.io.File
import javax.imageio.ImageIO

class ImageProcessor {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video2"


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

    fun setMatter(mode: MatterMode) {
        println("mode changed to $mode")
        matter = when (mode) {
            MatterMode.KMeans -> KMeansMatter(inputCam.grab())
            MatterMode.FaceDetection -> FaceDetectionMatter()
            MatterMode.BackgroundNegation -> BackgroundNegationMatter(inputCam.grab())
            MatterMode.OpenCV -> OpenCVMatter()
        }
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

    fun processImage() {
        val img = inputCam.grab()

        val mat = when {
            matter == null || background == null -> img
            else -> matter!!.changeBackground(img, background!!)
        }

        outputCam.write(Converter.convertToYUYV(mat))

        inputView.image = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(img), null)
        outputView.image = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(mat), null)
    }

    fun stopCameras() {
        inputCam.stop()
        outputCam.stop()
    }


    init {
        inputCam.start()
        outputCam.start()
    }
}