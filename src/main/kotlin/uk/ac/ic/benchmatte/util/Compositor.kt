package uk.ac.ic.benchmatte.util

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Scalar
import org.opencv.core.Size
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import java.awt.Color
import java.awt.color.ColorSpace.TYPE_RGB
import java.awt.image.BufferedImage
import java.awt.image.BufferedImage.TYPE_INT_RGB
import java.awt.image.DataBufferByte
import java.awt.image.DataBufferInt
import java.io.File
import javax.imageio.ImageIO
import kotlin.math.roundToInt

object Compositor {
    fun compose(backgroundSrc: String, foregroundSrc: String, maskSrc: String): Pair<Mat, Mat> {
        val background = loadFile(backgroundSrc)
        val foreground = loadFile(foregroundSrc)
        val mask = loadFile(maskSrc, CvType.CV_8U)

        return compose(background, foreground, mask)
    }

    fun compose(background: Mat, foreground: Mat, mask: Mat): Pair<Mat, Mat> {
        if (foreground.size() == mask.size()) {
            val (offset, targetSize) = calculateForegroundTransform(background, foreground)

            Imgproc.resize(foreground, foreground, targetSize)
            Imgproc.resize(mask, mask, targetSize)

            val resultMask = Mat(background.size(), CvType.CV_8U, Scalar(0.0, 0.0, 0.0))

            val newImage = Mat()
            background.copyTo(newImage)

            (0 until foreground.width()).forEach { x ->
                (0 until foreground.height()).forEach { y ->
                    val alpha = mask.get(y, x)[0] / 255.0
                    val blend = lerp(background.get(y, x + offset), foreground.get(y, x), alpha)

                    newImage.put(y, x + offset, blend[0], blend[1], blend[2])
                    resultMask.put(y, x + offset, alpha * 255.0)
                }
            }

            return Pair(newImage, resultMask)
        }

        return Pair(background, mask)
    }

    // This returns the X offset and the size
    fun calculateForegroundTransform(background: Mat, foreground: Mat): Pair<Int, Size> {
        var height = foreground.height().toDouble()
        var width = foreground.width().toDouble()
        if (height > background.height()) {
            val scaleFactor = background.height() / height
            height = background.height().toDouble()
            width *= scaleFactor
        }
        if (width > background.width()) {
            val scaleFactor = background.width() / width
            width = background.width().toDouble()
            height *= scaleFactor
        }
        return Pair((background.width() - width).roundToInt() / 2, Size(width, height))
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
        var buffer = ImageIO.read(File(src))

        if (buffer.type == 6) {
            val img = BufferedImage(buffer.width, buffer.height, TYPE_RGB)

            val g2d = img.createGraphics()
            g2d.setColor(Color.WHITE)
            g2d.fillRect(0, 0, img.getWidth(), img.getHeight())
            g2d.drawImage(buffer, 0, 0, null)
            g2d.dispose()
            buffer = img
        }

        val mat = Mat(buffer.height, buffer.width, type)

        mat.put(0, 0, (buffer.raster.dataBuffer as DataBufferByte).data)

        return mat
    }
}
