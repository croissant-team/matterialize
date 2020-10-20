package uk.ac.ic.benchmatte.util

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.imgproc.Imgproc
import java.awt.image.DataBufferByte
import java.io.File
import javax.imageio.ImageIO

object Compositor {
    fun compose(backgroundSrc: String, foregroundSrc: String, maskSrc: String): Mat {
        val background = loadFile(backgroundSrc)
        val foreground = loadFile(foregroundSrc)
        val mask = loadFile(maskSrc, CvType.CV_8U)

        return compose(background, foreground, mask)
    }

    fun compose(background: Mat, foreground: Mat, mask: Mat): Mat {
        if (foreground.size() == mask.size()) {
            Imgproc.resize(background, background, foreground.size())

            (0 until foreground.width()).forEach { x ->
                (0 until foreground.height()).forEach { y ->
                    val alpha = mask.get(y, x)[0] / 255.0
                    val blend = lerp(background.get(y, x), foreground.get(y, x), alpha)

                    foreground.put(y, x, blend[0], blend[1], blend[2])
                }
            }
        }

        return foreground
    }

    fun lerp(background: DoubleArray, foreground: DoubleArray, alpha: Double): DoubleArray {
        return (background zip foreground).map { (bg, fg) ->
            when {
                alpha >= 1.0 -> fg
                alpha <= 0.0 -> bg
                else -> (1.0 - alpha) * bg + alpha * fg
            }
        }.toDoubleArray()
    }

    fun loadFile(src: String, type: Int = CvType.CV_8UC3): Mat {
        val buffer = ImageIO.read(File(src))

        val mat = Mat(buffer.height, buffer.width, type)
        mat.put(0, 0, (buffer.raster.dataBuffer as DataBufferByte).data)

        return mat
    }
}
