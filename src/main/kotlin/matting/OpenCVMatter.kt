package matting

import org.opencv.core.CvType.CV_8UC1
import org.opencv.core.Mat
import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.COLOR_BGR2Lab
import org.opencv.imgproc.Imgproc.COLOR_Lab2BGR
import org.opencv.video.BackgroundSubtractor
import org.opencv.video.Video
import smile.clustering.KMeans
import smile.clustering.kmeans
import smile.math.MathEx

class OpenCVMatter(): Matter {
    private val backgroundSubtractor = Video.createBackgroundSubtractorKNN()

    override fun backgroundMask(videoFrame: Mat): Mat {
        val mask = Mat(videoFrame.height(), videoFrame.width(), CV_8UC1)
        backgroundSubtractor.apply(videoFrame, mask)

        return mask
    }

}