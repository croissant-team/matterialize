import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam

import java.nio.*

fun main() {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video100"

    val WIDTH = 640
    val HEIGHT = 360

    val inputCam = OpenCVWebcam(INPUT_DEVICE, WIDTH, HEIGHT)
    val outputCam = FakeWebcam(OUTPUT_DEVICE, WIDTH, HEIGHT)

    inputCam.start()
    outputCam.start()

    println("Average FPS: ${inputCam.fps(100)}")

    while (true) {
        val start = System.currentTimeMillis()

        val frame = ByteArray(WIDTH * HEIGHT * 3)
        val img = inputCam.grab()

        (0 until WIDTH).forEach { x ->
            (0 until HEIGHT).forEach { y ->
                // each pixel is represented as 3 bytes
                val i = (y * WIDTH + x) * 3

                // img.getRGB gives us ARGB (4 bytes) format, so we take bytes 2, 3 and 4 for our image
                val buffer = ByteBuffer.allocate(4).putInt(img.getRGB(x, y)).array().drop(1)

                (0..2).forEach { frame[i + it] = buffer[it] }
            }
        }

        outputCam.write(frame)

        val end = System.currentTimeMillis()
        println("took ${end - start}ms")
    }

    inputCam.stop()
    outputCam.stop()
}