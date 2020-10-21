package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.THRESH_BINARY

class BackgroundNegationMatter(private val background: Mat) : Matter {
    override fun foregroundMask(videoFrame: Mat): Mat {
        val diff = Mat()
        val mask = Mat()

        Core.absdiff(videoFrame, background, diff)
        Imgproc.cvtColor(diff, mask, Imgproc.COLOR_BGR2GRAY)

        val threshold = 25.0
        val maxval = 255.0
        Imgproc.threshold(mask, mask, threshold, maxval, THRESH_BINARY)


        return mask
    }
}
