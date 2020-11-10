package uk.ac.ic.matterialize.gui

import javafx.scene.image.ImageView
import tornadofx.Controller
import uk.ac.ic.matterialize.matting.MatterMode
import kotlin.concurrent.thread

class MatterializeController : Controller() {

    val imageProcessor = ImageProcessor()

    private lateinit var inputView: ImageView
    private lateinit var outputView: ImageView

    private var running = true

    fun setInputImage(image: ImageView) {
        inputView = image
    }

    fun setOutputImage(image: ImageView) {
        outputView = image
    }

    fun applyNewBackground(inputValue: String) {
        imageProcessor.applyNewBackground(inputValue)
    }

    fun setMatter(mode: MatterMode) {
        imageProcessor.setMatter(mode)
    }

    fun stopCameras() {
        running = false
        imageProcessor.stopCameras()
        imageProcessor.webcamControls.enableAutomaticControls()
    }

    fun initialiseWebCamThread() {
        thread {
            while (running) {
                val start = System.currentTimeMillis()

                val images = imageProcessor.processImage()

                inputView.image = images.first
                outputView.image = images.second

                val end = System.currentTimeMillis()
                println("took ${end - start}ms")
            }
        }
    }

    fun getImageWidth(): Double {
        return imageProcessor.WIDTH.toDouble()
    }
}
