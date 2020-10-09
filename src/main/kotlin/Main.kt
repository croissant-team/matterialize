import org.bytedeco.javacv.CanvasFrame
import org.bytedeco.javacv.Java2DFrameConverter
import org.bytedeco.javacv.OpenCVFrameGrabber

fun main() {
    val grabber = OpenCVFrameGrabber(0)
    grabber.imageWidth = 640
    grabber.imageHeight = 360

    grabber.start()

    val f = CanvasFrame("Demo Camera")

    while (true) {
        val conv = Java2DFrameConverter()
        val img = conv.getBufferedImage(grabber.grab())

        f.showImage(img)
    }

    grabber.stop()
}