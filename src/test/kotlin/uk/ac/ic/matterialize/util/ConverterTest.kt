package uk.ac.ic.matterialize.util

import io.kotest.core.spec.style.ShouldSpec
import io.kotest.matchers.shouldBe
import org.opencv.core.CvType.CV_8UC3
import org.opencv.core.Mat

@ExperimentalUnsignedTypes
class ConverterTest : ShouldSpec({
    nu.pattern.OpenCV.loadLocally()

    val blue = ubyteArrayOf(255u, 0u, 0u).toByteArray()
    val green = ubyteArrayOf(0u, 255u, 0u).toByteArray()
    val red = ubyteArrayOf(0u, 0u, 255u).toByteArray()
    val white = ubyteArrayOf(255u, 255u, 255u).toByteArray()

    should("convert an BGR Mat to a YUYV ByteArray") {
        val mat2By2 = Mat.zeros(2, 2, CV_8UC3)
        mat2By2.put(0, 0, blue)
        mat2By2.put(0, 1, green)
        mat2By2.put(1, 0, red)
        mat2By2.put(1, 1, white)
        val yuyvBytearray = Converter.convertToYUYV(mat2By2)

        yuyvBytearray shouldBe ubyteArrayOf(40u, 239u, 144u, 34u, 81u, 90u, 235u, 127u).toByteArray()
    }
})
