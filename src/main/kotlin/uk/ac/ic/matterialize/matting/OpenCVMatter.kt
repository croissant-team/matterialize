package uk.ac.ic.matterialize.matting

import org.opencv.core.Mat
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.THRESH_BINARY
import org.opencv.video.Video

class OpenCVMatter() : Matter {
    private val backgroundSubtractor = Video.createBackgroundSubtractorKNN()
    private val fgMask = Mat()

    override fun foregroundMask(videoFrame: Mat): Mat {
        backgroundSubtractor.apply(videoFrame, fgMask)
        val mask = Mat()
        // FIXME this doesn't really work as colors come in
        // the threshold is there to make the mask binary (no gray)
        Imgproc.threshold(fgMask, mask, 127.0, 255.0, THRESH_BINARY)

        return mask
    }
}
