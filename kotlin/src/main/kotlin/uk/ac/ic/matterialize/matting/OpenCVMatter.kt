package matting

import org.opencv.core.Mat
import org.opencv.video.Video
import uk.ac.ic.matterialize.matting.Matter

class OpenCVMatter() : Matter {
    private val backgroundSubtractor = Video.createBackgroundSubtractorKNN()

    override fun backgroundMask(videoFrame: Mat): Mat {
        val mask = Mat()
        backgroundSubtractor.apply(videoFrame, mask)

        return mask
    }
}
