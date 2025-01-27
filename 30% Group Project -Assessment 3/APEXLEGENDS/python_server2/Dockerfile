FROM python:3.11

# Install MySQL connector
RUN pip install mysql-connector-python

# Copy server code
COPY server2.py /usr/src/server2.py

WORKDIR /usr/src

EXPOSE 2003

CMD ["python", "server2.py"]
