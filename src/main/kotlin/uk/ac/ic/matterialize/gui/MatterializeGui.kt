package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.control.Label
import javafx.scene.control.ToggleGroup
import javafx.scene.image.ImageView
import javafx.stage.FileChooser
import matting.OpenCVMatter
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Scalar
import tornadofx.*
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.camera.V4L2Lib
import uk.ac.ic.matterialize.matting.BackgroundNegationMatter
import uk.ac.ic.matterialize.matting.KMeansMatter
import uk.ac.ic.matterialize.matting.Matter
import java.awt.image.BufferedImage
import java.awt.image.DataBufferByte
import java.io.File
import java.net.URL
import javax.imageio.ImageIO
import kotlin.concurrent.thread
import kotlin.system.exitProcess

class MatterializeApp : App(WebcamView::class) {

    private val webcamViewController: WebcamViewController by inject()

    override fun stop() {
        webcamViewController.joinThreads()
        exitProcess(0)
    }
}

class WebcamViewController : Controller() {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video100"

    val WIDTH = 640
    val HEIGHT = 480

    val inputCam = OpenCVWebcam(INPUT_DEVICE, WIDTH, HEIGHT)
    val outputCam = FakeWebcam(OUTPUT_DEVICE, WIDTH, HEIGHT)

    var matter: Matter? = null
    var background = Mat(WIDTH, HEIGHT, CvType.CV_8UC3, Scalar(0.0, 0.0, 0.0))

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

    fun setMatter(mode: String) {
        println("mode changed to $mode")
        matter = when (mode) {
            "opencv" -> OpenCVMatter()
            "kmeans" -> KMeansMatter(inputCam.grab())
            "bgneg" -> BackgroundNegationMatter(inputCam.grab())
            else -> null
        }
    }

    fun joinThreads() {
        inputCam.stop()
        outputCam.stop()
        // terminate all threads
    }

    fun initialiseWebCamThread(image: ImageView) {
        thread {
//            println("Average FPS: ${inputCam.fps(100)}")

            while (true) {
                val start = System.currentTimeMillis()

                val img = inputCam.grab()

                val mat = when (matter) {
                    null -> img
                    else -> matter!!.changeBackground(img, background)
                }

                outputCam.write(V4L2Lib.convertToYUYV(mat))

                image.image = SwingFXUtils.toFXImage(OpenCVWebcam.convertToBufferedImage(img), null)

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

class WebcamView : View("Matterialize") {

    private val webcamViewController: WebcamViewController by inject()
    private val matterGroup = ToggleGroup()

    override val root = stackpane {
        setPrefSize(webcamViewController.getImageWidth(), 150.0)

        val label = Label("No image selected")

        vbox {
            imageview {
                webcamViewController.initialiseWebCamThread(this)
            }

            borderpane {
                top = label {
                    useMaxWidth = true
                }

                center = button("Select a background image") {
                    action {
                        val file = chooseFile("Select a background image", arrayOf(
                            FileChooser.ExtensionFilter("image", "*.jpg", "*.jpeg", "*.png", "*.gif")
                        ))
                        webcamViewController.applyNewBackground(file.firstOrNull()?.absolutePath ?: "err")
                        label.text = file.firstOrNull()?.name ?: label.text
                    }
                }
            }

            borderpane {
                top = label {
                    useMaxWidth = true
                }

                bottom = label {
                    useMaxWidth = true
                }

                center = label
            }

            borderpane {
                top = Label("Select a matter")

                center = vbox {
                    val buttons = mapOf(
                        "KMeans" to "kmeans",
                        "OpenCV" to "opencv",
                        "Background Negation" to "bgneg"
                    )

                    buttons.forEach { (text, name) ->
                        radiobutton(text, matterGroup) {
                            action {
                                if (isSelected) webcamViewController.setMatter(name)
                            }
                        }
                    }
                }
            }
        }
    }
}
