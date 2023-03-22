FROM alpine:latest as build

RUN apk update
RUN apk upgrade
RUN apk add go
WORKDIR /usr/local/share/iot-prometheus
COPY . .
RUN go get .
RUN go build -o iot-prom .

FROM alpine:latest
WORKDIR /usr/local/share/iot-prometheus
COPY --from=build /usr/local/share/iot-prometheus/iot-prom .
CMD [ "./iot-prom" ]



