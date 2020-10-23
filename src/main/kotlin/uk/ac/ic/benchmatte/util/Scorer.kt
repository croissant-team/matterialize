package uk.ac.ic.benchmatte.util

import org.opencv.core.Core
import org.opencv.core.Mat

object Scorer {
    // a is the actual, b is the result
    fun difference(a: Mat, b: Mat, threshold: Double = 127.0): Pair<Array<IntArray>, Mat> {
        if (a.size() != b.size()) return Pair(emptyArray(), Mat())

        val c = Mat()
        Core.absdiff(a, b, c)

        /*
        *      bB bW
        *   aB
        *   aW
        */
        val confusion = Array(2) { IntArray(2) }

        (0 until c.width()).forEach { x ->
            (0 until c.height()).forEach { y ->
                val aIndex = if (a.get(y, x)[0] > threshold) 1 else 0
                val bIndex = if (b.get(y, x)[0] > threshold) 1 else 0

                confusion[aIndex][bIndex]++
            }
        }

        return Pair(confusion, c)
    }

    fun percentage(a: Mat, b: Mat, threshold: Double = 127.0): Double {
        val (confusion, c) = difference(a, b, threshold)

        if (confusion.isEmpty()) return -1.0

        return 1.0 - ((confusion[0][1] + confusion[1][0]).toDouble() / (c.width() * c.height()))
    }
}
