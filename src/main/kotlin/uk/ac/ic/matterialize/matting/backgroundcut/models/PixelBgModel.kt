package uk.ac.ic.matterialize.matting.backgroundcut

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Size
import kotlin.math.exp

class PixelBgModel(bgImage: Image) {
    private val perPixelVariances = bgImage.getPixelVariances()
    private val flatBgImage = bgImage.flattened

    fun pdf(l2: Double, det: Double): Double {
        val x = -1 * l2 / 2
        return exp(x) / kotlin.math.sqrt(twoPiCubed * det)
    }

    fun perPixelProbs(image: FlatImage): Probs {
        val probs = Mat(Size(1.0, image.numPixels().toDouble()), CvType.CV_64FC1)

        for (pixel in 0 until image.numPixels()) {
            var l2 = 0.0
            var det = 1.0
            for (channel in 0 until 3) {
                val delta = image.get(pixel, channel).toDouble() - (flatBgImage.get(pixel, channel)).toDouble()
                l2 += (delta * delta) / perPixelVariances.at(pixel, channel)
                det *= perPixelVariances.at(pixel, channel)
            }

            probs.put(pixel, 0, pdf(l2, det))
        }

        return Probs(probs)
    }
}