package uk.ac.ic.matterialize.camera

import org.bytedeco.javacv.Java2DFrameConverter
import org.bytedeco.javacv.OpenCVFrameGrabber
import java.awt.image.BufferedImage

class OpenCVWebcam(private val device: Int, private val width: Int, private val height: Int) {
    private lateinit var grabber: OpenCVFrameGrabber
    private val conv = Java2DFrameConverter()

    fun start() {
        grabber = OpenCVFrameGrabber(device)
        grabber.imageWidth = width
        grabber.imageHeight = height

        grabber.start()
    }

    fun stop() {
        grabber.stop()
    }

    fun grab(): BufferedImage {
        return conv.getBufferedImage(grabber.grab())
    }

    fun fps(samples: Int): Double {
        println("Taking $samples samples to estimate FPS...")
        var sum = 0L

        (0 until samples).forEach {
            val start = System.currentTimeMillis()
            grabber.grab()
            val end = System.currentTimeMillis()

            sum += end - start
        }

        return 1000 / (sum / samples.toDouble())
    }
}