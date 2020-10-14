import org.opencv.imgcodecs.Imgcodecs
import org.opencv.imgproc.Imgproc
import uk.ac.ic.matterialize.camera.FakeWebcam
import uk.ac.ic.matterialize.camera.OpenCVWebcam
import uk.ac.ic.matterialize.camera.V4L2Lib

fun main() {
    val INPUT_DEVICE = 0

    // might need to change output device depending on configuration
    // dummy devices can be seen by by running `v4l2-ctl --list-devices`
    val OUTPUT_DEVICE = "/dev/video2"

    val WIDTH = 640
    val HEIGHT = 480

    val inputCam = OpenCVWebcam(INPUT_DEVICE, WIDTH, HEIGHT)
    val outputCam = FakeWebcam(OUTPUT_DEVICE, WIDTH, HEIGHT)

    inputCam.start()
    outputCam.start()

    println("Average FPS: ${inputCam.fps(100)}")

    while (true) {
        val start = System.currentTimeMillis()

        val img = inputCam.grab()

        outputCam.write(V4L2Lib.convertToYUYV(OpenCVWebcam.convertToBufferedImage(img)))

        val end = System.currentTimeMillis()
        println("took ${end - start}ms")
    }

    inputCam.stop()
    outputCam.stop()
}