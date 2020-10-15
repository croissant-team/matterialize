package uk.ac.ic.matterialize.matting

import org.opencv.core.*
import org.opencv.imgproc.Imgproc
import org.opencv.imgproc.Imgproc.*
import org.opencv.objdetect.CascadeClassifier

class FloatingHead() : Matter {
    private val classifier = CascadeClassifier("src/main/resources/haarcascade_frontalface_default.xml")

    override fun foregroundMask(videoFrame: Mat): Mat {
        val mask = Mat(videoFrame.size(), videoFrame.type(), Scalar(0.0, 0.0, 0.0))
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
                (it.y .. mask.height()).forEach { j ->
                    mask.put(j, i, 255.0, 255.0, 255.0)
                }
            }
        }

        return mask
    }
}
