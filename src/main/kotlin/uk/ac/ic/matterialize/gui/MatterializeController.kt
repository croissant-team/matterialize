package uk.ac.ic.matterialize.gui

import javafx.scene.image.ImageView
import tornadofx.Controller
import uk.ac.ic.matterialize.matting.MatterMode
import kotlin.concurrent.thread

class MatterializeController : Controller() {

    val imageProcessor = ImageProcessor()

    private var running = true

    fun setInputImage(image: ImageView) {
        imageProcessor.setInputImage(image)
    }

    fun setOutputImage(image: ImageView) {
        imageProcessor.setOutputImage(image)
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
    }

    fun initialiseWebCamThread() {
        thread {
            while (running) {
                val start = System.currentTimeMillis()

                imageProcessor.processImage()

                val end = System.currentTimeMillis()
                println("took ${end - start}ms")
            }
        }
    }

    fun getImageWidth(): Double {
        return imageProcessor.WIDTH.toDouble()
    }


}
