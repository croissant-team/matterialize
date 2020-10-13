package uk.ac.ic.matterialize.camera

import org.opencv.core.Mat
import java.awt.image.BufferedImage
import java.nio.ByteBuffer

class V4L2Lib {
    companion object {
        init {
            System.loadLibrary("fakecam")
        }

        // roughly reproduces for Kotlin the C++ code found at http://jevois.org/doc/RawImageOps_8C_source.html#l01038

        @JvmStatic
        @ExperimentalUnsignedTypes
        fun convertToYUYV(src: BufferedImage): ByteArray {
            val height = src.height
            val width = src.width
            val outputData = UByteArray(width * height * 2)

            val outlineSize = width * 2

            (0 until height).forEach { y ->
                val outoff = y * outlineSize;
                (0 until width step 2).forEach { x ->
                    val p1 = ByteBuffer.allocate(4).putInt(src.getRGB(x, y)).array()
                    val p2 = ByteBuffer.allocate(4).putInt(src.getRGB(x + 1, y)).array()

                    val yuyv = BGRToYUYV(p1[3], p1[2], p1[1], p2[3], p2[2], p2[1])

                    val indexDst = outoff + x * 2

                    (0 until 4).forEach { outputData[indexDst + it] = yuyv[it] }
                }
            }

            return outputData.toByteArray()
        }

        @JvmStatic
        @ExperimentalUnsignedTypes
        fun convertToYUYV(src: Mat): ByteArray {
            val height = src.height()
            val width = src.width()
            val inputData = ByteArray(width * height * 3)
            src.get(0, 0, inputData)
            val outputData = UByteArray(width * height * 2)

            val inlineSize = width * 3
            val outlineSize = width * 2

            (0 until height).forEach { y ->
                val inoff = y * inlineSize;
                val outoff = y * outlineSize;
                (0 until width step 2).forEach { x ->
                    val indexSrc = inoff + x * 3

                    val yuyv = BGRToYUYV(
                        inputData[indexSrc + 0],
                        inputData[indexSrc + 1],
                        inputData[indexSrc + 2],
                        inputData[indexSrc + 3],
                        inputData[indexSrc + 4],
                        inputData[indexSrc + 5]
                    )

                    val indexDst = outoff + x * 2

                    (0 until 4).forEach { outputData[indexDst + it] = yuyv[it] }
                }
            }

            return outputData.toByteArray()
        }

        @JvmStatic
        @ExperimentalUnsignedTypes
        fun BGRToYUYV(b1: Byte, g1: Byte, r1: Byte, b2: Byte, g2: Byte, r2: Byte): UByteArray {
            val b1f = b1.toUByte().toFloat()
            val g1f = g1.toUByte().toFloat()
            val r1f = r1.toUByte().toFloat()
            val b2f = b2.toUByte().toFloat()
            val g2f = g2.toUByte().toFloat()
            val r2f = r2.toUByte().toFloat()

            return floatArrayOf(
                (0.257F * r1f) + (0.504F * g1f) + (0.098F * b1f) + 16.0F,
                -(0.148F * r1f) - (0.291F * g1f) + (0.439F * b1f) + 128.0F,
                (0.257F * r2f) + (0.504F * g2f) + (0.098F * b2f) + 16.0F,
                (0.439F * r2f) - (0.368F * g2f) - (0.071F * b2f) + 128.0F
            ).map { it.toUInt().toUByte() }.toUByteArray()
        }

    }

    external fun open(device: String, width: Int, height: Int): Int

    external fun writeFrame(dev: Int, frame: ByteArray?): Boolean

    external fun close(dev: Int): Int
}