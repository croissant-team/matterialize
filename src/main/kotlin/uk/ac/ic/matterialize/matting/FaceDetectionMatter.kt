package uk.ac.ic.matterialize.matting

import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.MatOfRect
import org.opencv.core.Scalar
import org.opencv.core.Size
import org.opencv.imgproc.Imgproc.COLOR_BGR2GRAY
import org.opencv.imgproc.Imgproc.cvtColor
import org.opencv.objdetect.CascadeClassifier

class FaceDetectionMatter() : Matter {
    private val classifier = CascadeClassifier("src/main/resources/haarcascade_frontalface_default.xml")

    override fun foregroundMask(videoFrame: Mat): Mat {
        val mask = Mat(videoFrame.size(), CvType.CV_8U, Scalar(0.0))
        val gray = Mat()
        cvtColor(videoFrame, gray, COLOR_BGR2GRAY)
        val faces = MatOfRect()
        classifier.detectMultiScale(
            gray,
            faces,
            1.05,
            8,
            0,
            Size(50.0, 50.0),
            Size(150.0, 150.0)
        )

        faces.toArray().forEach {
            ((it.x - it.width / 2) until it.x + (3 * it.width) / 2).forEach { i ->
                (it.y..mask.height()).forEach { j ->
                    mask.put(j, i, 255.0)
                }
            }
        }

        return mask
    }
}
