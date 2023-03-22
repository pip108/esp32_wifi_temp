package main

import (
	"log"

	"github.com/gin-gonic/gin"
	"github.com/prometheus/client_golang/prometheus"
	"github.com/prometheus/client_golang/prometheus/promhttp"
)

type TempData struct {
	Sensor string  `json:"sensor"`
	Value  float32 `json:"value"`
}

type metrics struct {
	temp *prometheus.GaugeVec
}

var temps = make(map[string][]TempData)

func main() {
	router := gin.Default()

	m := TempMetrics()

	router.POST("/temp", func(c *gin.Context) {
		var data TempData
		if err := c.BindJSON(&data); err != nil {
			var str string
			log.Printf("[%s] %s", c.Bind(&str), err.Error())
			return
		}
		log.Printf("[%s]: %.2f °C", data.Sensor, data.Value)
		m.temp.With(prometheus.Labels{"sensor": data.Sensor}).Set(float64(data.Value))
		temps[data.Sensor] = append(temps[data.Sensor], data)
		c.Status(201)
	})
	router.GET("/metrics", promHandler())
	log.Fatal(router.Run("0.0.0.0:8080"))
}

func TempMetrics() *metrics {
	m := metrics{
		temp: prometheus.NewGaugeVec(prometheus.GaugeOpts{
			Name: "temperature",
			Help: "Temperature for sensor in degreess Celsius",
		},
			[]string{"sensor"}),
	}
	defaultRegistry := prometheus.NewRegistry()
	prometheus.DefaultRegisterer = defaultRegistry
	prometheus.DefaultGatherer = defaultRegistry

	prometheus.MustRegister(m.temp)

	return &m
}

func promHandler() gin.HandlerFunc {
	h := promhttp.HandlerFor(prometheus.DefaultGatherer, promhttp.HandlerOpts{})
	return func(c *gin.Context) {
		h.ServeHTTP(c.Writer, c.Request)
	}
}
