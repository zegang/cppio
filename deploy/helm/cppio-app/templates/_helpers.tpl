{{/* Helper template for full resource name */}}
{{- define "cppio-app.fullname" -}}
{{- if .Values.fullnameOverride -}}
{{ .Values.fullnameOverride | trunc 63 | trimSuffix "-" }}
{{- else -}}
{{- $name := default .Chart.Name .Values.nameOverride -}}
{{- printf "%s-%s" .Release.Name $name | trunc 63 | trimSuffix "-" }}
{{- end -}}
{{- end -}}

{{/* Daemon service name */}}
{{- define "cppio-app.daemonServiceName" -}}
{{ include "cppio-app.fullname" . }}-daemon
{{- end -}}

{{/* Daemon app label */}}
{{- define "cppio-app.daemonName" -}}
{{ include "cppio-app.fullname" . }}-daemon
{{- end -}}

{{/* Daemon app label */}}
{{- define "cppio-app.daemonFullname" -}}
{{ include "cppio-app.fullname" . }}-daemon
{{- end -}}

{{/* CLI app label */}}
{{- define "cppio-app.cliName" -}}
{{ include "cppio-app.fullname" . }}-cli
{{- end -}}

{{/* CLI deployment name */}}
{{- define "cppio-app.cliFullname" -}}
{{ include "cppio-app.fullname" . }}-cli
{{- end -}}

{{/* Chart label helper */}}
{{- define "cppio-app.chart" -}}
{{ .Chart.Name }}-{{ .Chart.Version }}
{{- end -}}
