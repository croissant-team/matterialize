package uk.ac.ic.matterialize.camera

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.videoio.VideoCapture
import org.opencv.videoio.Videoio.CAP_V4L
import java.awt.image.BufferedImage
import java.io.ByteArrayInputStream
import javax.imageio.ImageIO


class OpenCVWebcam(private val device: Int, private val width: Int, private val height: Int) {
    companion object {
        init {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME)
        }
    }

    private var capture: VideoCapture? = null

    fun start() {
        capture = VideoCapture(device, CAP_V4L)
    }

    fun stop() {
        capture?.release()
        capture = null
    }

    // TODO convert into utility function and move away
    fun grabBuf(): BufferedImage {
        val mat = grabMat()
        val buffer = ByteArray(mat.width() * mat.height() * mat.channels())
        mat.get(0, 0, buffer)
        val input = ByteArrayInputStream(buffer)

        return ImageIO.read(input)
    }

    fun grabMat(): Mat {
        val mat = Mat()
        // TODO add proper error
        capture!!.read(mat)
        return mat
    }

    fun fps(samples: Int): Double {
        println("Taking $samples samples to estimate FPS...")
        var sum = 0L

        (0 until samples).forEach {
            val start = System.currentTimeMillis()
            grabMat()
            val end = System.currentTimeMillis()

            sum += end - start
        }

        return 1000 / (sum / samples.toDouble())
    }
}