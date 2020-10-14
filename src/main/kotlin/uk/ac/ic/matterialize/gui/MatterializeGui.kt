package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.control.Label
import javafx.scene.image.ImageView
import javafx.scene.image.WritableImage
import javafx.stage.FileChooser
import org.bytedeco.javacv.Frame
import org.bytedeco.javacv.Java2DFrameConverter
import org.bytedeco.javacv.OpenCVFrameConverter
import org.bytedeco.javacv.OpenCVFrameGrabber
import org.bytedeco.opencv.global.opencv_core.cvFlip
import tornadofx.*
import java.awt.image.BufferedImage
import java.util.concurrent.Executors
import kotlinx.coroutines.*
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
    private val java2DFrameConverter: Java2DFrameConverter = Java2DFrameConverter()
    val grabber = OpenCVFrameGrabber(0)

    fun applyNewBackground(inputValue: String) {
        println("Image '$inputValue' has been selected")
    }

    fun joinThreads() {
        // terminate all threads
    }

    fun initialiseWebCamThread(image: ImageView) {
        val grabberConverter = OpenCVFrameConverter.ToIplImage()

//        Executors.newSingleThreadExecutor().execute {
//        GlobalScope.launch {
//            println("hello")
//        }

        thread {
            while (true) {
                val iplImage = grabberConverter.convert(grabber.grabFrame())
                cvFlip(iplImage, iplImage, 1)
                val frame = grabberConverter.convert(iplImage)
                image.image = frameToImage(frame)
            }
        }
    }

    fun getImageWidth(): Double {
        return grabber.grabFrame().imageWidth.toDouble()
    }

    private fun frameToImage(frame: Frame): WritableImage {
        val bufferedImage: BufferedImage = java2DFrameConverter.getBufferedImage(frame)
        return SwingFXUtils.toFXImage(bufferedImage, null)
    }

    init {
        grabber.start()
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
