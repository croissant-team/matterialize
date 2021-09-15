package uk.ac.ic.matterialize.util

import org.opencv.core.Mat

object OpenCV {
    fun load() {
        nu.pattern.OpenCV.loadLocally()
    }

    fun Mat.asDoubleArray(): DoubleArray {
        assert(this.type() >= 0)
        val data = DoubleArray(this.rows() * this.cols() * this.channels())
        this.get(0, 0, data)
        return data
    }
}
