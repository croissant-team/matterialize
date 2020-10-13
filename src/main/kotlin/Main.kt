import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.camera.V4L2Lib

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

        val img = inputCam.grabMat()

        outputCam.write(V4L2Lib.convertToYUYV(img))

        val end = System.currentTimeMillis()
        println("took ${end - start}ms")
    }

    inputCam.stop()
    outputCam.stop()
}