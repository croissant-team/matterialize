package uk.ac.ic.matterialize.camera

import org.bytedeco.javacv.Java2DFrameConverter
import org.bytedeco.javacv.OpenCVFrameConverter
import org.bytedeco.javacv.OpenCVFrameGrabber
import org.opencv.core.Mat
import org.opencv.core.Core
import java.awt.image.BufferedImage

class OpenCVWebcam(private val device: Int, private val width: Int, private val height: Int) {
    companion object {
        init {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME)
        }
    }

    private lateinit var grabber: OpenCVFrameGrabber
    private val bufConv = Java2DFrameConverter()
    private val matConv = OpenCVFrameConverter.ToOrgOpenCvCoreMat()

    fun start() {
        grabber = OpenCVFrameGrabber(device)
        grabber.imageWidth = width
        grabber.imageHeight = height

        grabber.start()
    }

    fun stop() {
        grabber.stop()
    }

    fun grabBuf(): BufferedImage {
        return bufConv.getBufferedImage(grabber.grab())
    }

    fun grabMat(): Mat {
        return matConv.convert(grabber.grab())
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