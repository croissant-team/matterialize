package uk.ac.ic.matterialize.matting

import org.opencv.core.Core
import org.opencv.core.Mat
import org.opencv.core.Scalar

interface Matter {

    fun backgroundMask(videoFrame: Mat): Mat

    fun greenscreen(videoFrame: Mat): Mat {
        val greenscreen = Mat(videoFrame.size(), videoFrame.type(), Scalar(0.0, 255.0, 0.0))
        return changeBackground(videoFrame, greenscreen)
    }

    fun changeBackground(videoFrame: Mat, newBackground: Mat): Mat {
        val bgMask = backgroundMask(videoFrame)
        val fgMask = Mat()
        Core.bitwise_not(bgMask, fgMask)
        val bg = Mat()
        val fg = Mat()
        videoFrame.copyTo(fg, bgMask)
        newBackground.copyTo(bg, fgMask)

        val result = Mat()
        Core.add(fg, bg, result)

        return result
    }

    fun removeBackground(videoFrame: Mat): Mat {
        val result = Mat()
        val fgMask = backgroundMask(videoFrame)
        videoFrame.copyTo(result, fgMask)

        return result
    }
}
