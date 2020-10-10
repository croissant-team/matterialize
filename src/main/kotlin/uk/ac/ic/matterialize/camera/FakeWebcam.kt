package uk.ac.ic.matterialize.camera

class FakeWebcam(private val device: String, private val width: Int, private val height: Int) {
    private val v4l2 = V4L2Lib()
    private var dev = -1

    fun start(): Int {
        dev = v4l2.open(device, width, height)
        return dev
    }

    fun stop() {
        v4l2.close(dev)
    }

    fun write(frame: ByteArray?): Boolean {
        return v4l2.writeFrame(dev, frame)
    }
}