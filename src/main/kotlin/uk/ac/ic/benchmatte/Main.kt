package uk.ac.ic.benchmatte

import uk.ac.ic.benchmatte.util.Benchmark

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val benchmark = Benchmark("", "", "")

    benchmark.setup()
    benchmark.export()

    println(benchmark.run())
}
