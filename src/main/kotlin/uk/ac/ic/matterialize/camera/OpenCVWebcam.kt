package uk.ac.ic.matterialize.camera

import org.opencv.core.Mat
import org.opencv.core.MatOfByte
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.videoio.VideoCapture
import org.opencv.videoio.Videoio.CAP_V4L
import java.awt.image.BufferedImage
import java.io.ByteArrayInputStream
import javax.imageio.ImageIO

class OpenCVWebcam(private val device: Int, private val width: Int, private val height: Int) {
    companion object {
        init {
            nu.pattern.OpenCV.loadLocally()
        }

        fun convertToBufferedImage(mat: Mat): BufferedImage {
            val mob = MatOfByte()
            Imgcodecs.imencode(".png", mat, mob)
            return ImageIO.read(ByteArrayInputStream(mob.toArray()))
        }
    }

    private var capture: VideoCapture? = null

    fun start() {
        capture = VideoCapture()
        capture!!.open(device, CAP_V4L)
    }

    fun stop() {
        capture?.release()
        capture = null
    }

    fun grab(): Mat {
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
            grab()
            val end = System.currentTimeMillis()

            sum += end - start
        }

        return 1000 / (sum / samples.toDouble())
    }
}