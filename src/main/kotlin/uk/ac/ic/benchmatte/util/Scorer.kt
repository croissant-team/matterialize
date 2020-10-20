package uk.ac.ic.benchmatte.util

import org.opencv.core.Core
import org.opencv.core.Mat

object Scorer {
    fun difference(a: Mat, b: Mat, threshold: Double = 127.0): Pair<Int, Mat> {
        if (a.size() != b.size()) return Pair(-1, Mat())

        val c = Mat()
        Core.absdiff(a, b, c)

        var delta = 0

        (0 until a.width()).forEach { x ->
            (0 until a.height()).forEach { y ->
                if (c.get(y, x)[0] > threshold) delta++
            }
        }

        return Pair(delta, c)
    }

    fun percentage(a: Mat, b: Mat, threshold: Double = 127.0): Double {
        val (delta, c) = difference(a, b, threshold)

        if (delta == -1) return -1.0

        return 1.0 - (delta.toDouble() / (c.width() * c.height()))
    }
}
