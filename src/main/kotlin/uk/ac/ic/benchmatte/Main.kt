package uk.ac.ic.benchmatte

import uk.ac.ic.benchmatte.util.Benchmark

fun main() {
    nu.pattern.OpenCV.loadLocally()

    val benchmark = Benchmark("masks", "bg", "fg")

    benchmark.setup()
    benchmark.export()

    val results = benchmark.run()

    results.forEach { imgResults ->
        imgResults.forEach { (mode, matrix, time) ->
            println(mode)
            println("Confusion matrix:")
            println("                  Predicted")
            println("               bg     |     fg")
            println("            ---------------------")
            println("         bg | ${matrix[0][0]}    ${matrix[0][1]}  |")
            println("Actual      |                   |")
            println("         fg | ${matrix[1][0]}    ${matrix[1][1]}  |")
            println("            ---------------------")
            println("Time taken: ${time}ms\n")
        }

        println("\n")
    }
}
