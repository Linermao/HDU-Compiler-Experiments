docker compose up -d

container_id=$(docker ps -qf "name=compiler")

echo container_id: $container_id

docker exec -it "$container_id" bash