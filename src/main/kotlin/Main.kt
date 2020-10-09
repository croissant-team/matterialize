import org.bytedeco.javacv.Java2DFrameConverter
import org.bytedeco.javacv.OpenCVFrameGrabber
import java.nio.*

fun main() {
    val cam = FakeCamera()

    val INPUT_DEVICE = 0
    val OUTPUT_DEVICE = "/dev/video100"

    val WIDTH = 640
    val HEIGHT = 360

    val grabber = OpenCVFrameGrabber(INPUT_DEVICE)
    grabber.imageWidth = WIDTH
    grabber.imageHeight = HEIGHT
    grabber.start()

    val dev = cam.open(OUTPUT_DEVICE, WIDTH, HEIGHT)
    val conv = Java2DFrameConverter()

    while (true) {
        val start = System.currentTimeMillis()

        val frame = ByteArray(WIDTH * HEIGHT * 3)
        val img = conv.getBufferedImage(grabber.grab())

        (0 until WIDTH).forEach { x ->
            (0 until HEIGHT).forEach { y ->
                // each pixel is represented as 3 bytes
                val i = (y * WIDTH + x) * 3

                // img.getRGB gives us ARGB format, so we take bytes 2, 3 and 4 for our image
                val buffer = ByteBuffer.allocate(4).putInt(img.getRGB(x, y)).array()

                frame[i] = buffer[1]     // R
                frame[i + 1] = buffer[2] // G
                frame[i + 2] = buffer[3] // B
            }
        }

        cam.writeFrame(dev, frame)

        val end = System.currentTimeMillis()
        println("took ${end - start}ms")
    }
}