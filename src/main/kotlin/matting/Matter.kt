package matting

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.core.Scalar

interface Matter {

    fun backgroundMask(videoFrame: Mat): Mat

    fun greenscreen(videoFrame: Mat): Mat {
        // FIXME doesn't really work currently
        // TODO there is probably a simpler way: find it
        val result = Mat()
        val bgMask = backgroundMask(videoFrame)
        val fgMask = Mat()
        Core.bitwise_not(bgMask, fgMask)
        val greenscreen = Mat(videoFrame.size(), videoFrame.type(), Scalar(0.0, 0.0, 0.0))
        val bg = Mat()
        val fg = Mat()
        videoFrame.copyTo(fg, bgMask)
        greenscreen.copyTo(bg, fgMask)

        Core.add(fg, bg, result)

        return result
    }

    fun removeBackground(videoFrame: Mat, background: Mat): Mat {
        val result = Mat()
        val mask = backgroundMask(videoFrame)
        videoFrame.copyTo(result, mask)

        return result
    }
}
