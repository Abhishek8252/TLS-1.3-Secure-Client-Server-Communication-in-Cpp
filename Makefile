//2. TLS_AES_Comm Makefile

# Compiler and Flags
CXX = g++
LIBS = -lssl -lcrypto

# Target Binaries
SERVER = server
CLIENT = client

# Default: Build both
all: $(SERVER) $(CLIENT)

# Compile Server
$(SERVER): server.cpp
	@$(CXX) $(CXXFLAGS) server.cpp -o $(SERVER) $(LIBS)

# Compile Client
$(CLIENT): client.cpp
	@$(CXX) $(CXXFLAGS) client.cpp -o $(CLIENT) $(LIBS)

# Optional: Generate Certificates
certs:
	@echo "Generating Certificates..."
	openssl genrsa -out gr_ca_key.pem 2048
	openssl req -x509 -new -nodes -key gr_ca_key.pem -sha256 -days 3650 -out gr_ca_cert1.pem -subj "/CN=MyCustomCA"
	openssl genrsa -out server.key 2048
	openssl req -new -key server.key -out server.csr -subj "/CN=localhost"
	openssl x509 -req -in server.csr -CA gr_ca_cert1.pem -CAkey gr_ca_key.pem -CAcreateserial -out server.crt -days 365 -sha256
	@echo "Certificates created."

# Clean up binaries
clean:
	@rm -f $(SERVER) $(CLIENT) *.srl server.csr

# Remove everything including certs
distclean: clean
	@rm -f *.pem *.crt *.key
