package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.image.WritableImage
import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.matting.backgroundcut.BackgroundCut
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

    fun setMatter(mode: MatterMode) {
        println("mode changed to $mode")
        matter = when (mode) {
            MatterMode.KMeans -> KMeansMatter(inputCam.grab())
            MatterMode.FaceDetection -> FaceDetectionMatter()
            MatterMode.BackgroundNegation -> BackgroundNegationMatter(inputCam.grab())
            MatterMode.OpenCV -> OpenCVMatter()
            MatterMode.BackgroundCut -> BackgroundCut(inputCam.grab())
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

    fun processImage(): Pair<WritableImage, WritableImage> {
        val img = inputCam.grab()

        val mat = when {
            matter == null || background == null -> img
            else -> matter!!.changeBackground(img, background!!)
        }

        outputCam.write(Converter.convertToYUYV(mat))

        val inputImage = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(img), null)
        val outputImage = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(mat), null)

        return Pair(inputImage, outputImage)
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
