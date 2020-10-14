package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.control.Label
import javafx.scene.image.ImageView
import javafx.stage.FileChooser
import matting.KMeansMatter
import matting.OpenCVMatter
import org.opencv.core.Core
import org.opencv.core.CvType
import org.opencv.core.Mat
import tornadofx.*
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.camera.V4L2Lib
import java.awt.image.DataBufferByte
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

class WebcamViewController: Controller() {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video100"

    val WIDTH = 640
    val HEIGHT = 480

    val inputCam = OpenCVWebcam(INPUT_DEVICE, WIDTH, HEIGHT)
    val outputCam = FakeWebcam(OUTPUT_DEVICE, WIDTH, HEIGHT)

    fun applyNewBackground(inputValue: String) {
        println("Image '$inputValue' has been selected")
    }

    fun joinThreads() {
        inputCam.stop()
        outputCam.stop()
        // terminate all threads
    }

    fun initialiseWebCamThread(image: ImageView) {
        thread {
//            println("Average FPS: ${inputCam.fps(100)}")

            val bi = ImageIO.read(URL("https://upload.wikimedia.org/wikipedia/commons/f/fc/EAM_Nuvolari_S1_640x480.jpg"))
            val m = Mat(bi.height, bi.width, CvType.CV_8UC3)
            m.put(0, 0, (bi.raster.dataBuffer as DataBufferByte).data)
            val matter = OpenCVMatter()

            while (true) {
                val start = System.currentTimeMillis()

                val img = inputCam.grab()

                outputCam.write(V4L2Lib.convertToYUYV(matter.backgroundMask(img)))

//                Core.flip(img, img, 1)

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
                        // use tornado file select as it lets you filter by extension
                        val fileChooser = FileChooser()
                        val file = fileChooser.showOpenDialog(null)
                        webcamViewController.applyNewBackground(file?.absolutePath ?: "err")
                        label.text = file?.name ?: label.text
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
        }
    }
}
