class FakeCamera {
    companion object {
        init {
            System.loadLibrary("fakecam")
        }
    }

    external fun open(device: String, width: Int, height: Int): Int

    external fun writeFrame(dev: Int, frame: ByteArray?): Boolean

    external fun close(dev: Int): Int
}