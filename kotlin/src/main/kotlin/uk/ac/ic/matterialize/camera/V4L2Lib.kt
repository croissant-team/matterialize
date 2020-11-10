package uk.ac.ic.matterialize.camera

class V4L2Lib {
    companion object {
        init {
            System.loadLibrary("fakecam")
        }

        // roughly reproduces for Kotlin the C++ code found at http://jevois.org/doc/RawImageOps_8C_source.html#l01038
    }

    external fun open(device: String, width: Int, height: Int): Int

    external fun writeFrame(dev: Int, frame: ByteArray?): Boolean

    external fun close(dev: Int): Int
}
