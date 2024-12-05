docker compose up -d

container_id=$(docker ps -qf "name=compiler")

docker exec -it "$container_id" bash