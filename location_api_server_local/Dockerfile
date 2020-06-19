FROM tiangolo/uwsgi-nginx-flask:python3.8

RUN pip install pipenv \
    && mkdir /setup
COPY Pipfile /setup
RUN cd /setup \
    && pipenv lock --requirements > requirements.txt \
    && pip install -r /setup/requirements.txt \
    && rm -rf /setup

COPY ./app /app