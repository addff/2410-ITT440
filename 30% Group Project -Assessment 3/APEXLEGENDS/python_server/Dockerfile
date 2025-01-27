FROM python:3.11

# Install MySQL connector
RUN pip install mysql-connector-python

# Copy server code
COPY server.py /usr/src/server.py

WORKDIR /usr/src

EXPOSE 2002

CMD ["python", "server.py"]
