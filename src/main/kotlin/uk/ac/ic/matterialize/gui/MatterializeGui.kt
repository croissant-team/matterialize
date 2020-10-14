package uk.ac.ic.matterialize.gui

import javafx.embed.swing.SwingFXUtils
import javafx.scene.control.Label
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
import kotlin.system.exitProcess

class MatterializeApp : App(WebcamView::class) {

    override fun stop() {
        exitProcess(0)
    }
}

class WebcamView : View("Matterialize") {

    private val java2DFrameConverter: Java2DFrameConverter = Java2DFrameConverter()

    override val root = stackpane {
        val grabber = OpenCVFrameGrabber(0)
        grabber.start()
        setPrefSize(grabber.grabFrame().imageWidth.toDouble(), 150.0)

        val label = Label("No image selected")

        vbox {
            imageview {
                val grabberConverter = OpenCVFrameConverter.ToIplImage()

                Executors.newSingleThreadExecutor().execute {
                    while (true) {
                        val iplImage = grabberConverter.convert(grabber.grabFrame())
                        cvFlip(iplImage, iplImage, 1)
                        val frame = grabberConverter.convert(iplImage)
                        this.image = frameToImage(frame)
                    }
                }
            }

            borderpane {
                top = label {
                    useMaxWidth = true
                }

                center = button("Select a background image") {
                    action {
                        val fileChooser = FileChooser()
                        val file = fileChooser.showOpenDialog(null)
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

    private fun frameToImage(frame: Frame): WritableImage {
        val bufferedImage: BufferedImage = java2DFrameConverter.getBufferedImage(frame)
        return SwingFXUtils.toFXImage(bufferedImage, null)
    }
}
